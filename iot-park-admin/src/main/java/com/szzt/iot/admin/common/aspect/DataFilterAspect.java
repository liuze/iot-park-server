/**
 * Copyright (c) 2019 证通电子 All rights reserved.
 * <p>
 * https://www.szzt.com.cn
 * <p>
 * 版权所有，侵权必究！
 */

package com.szzt.iot.admin.common.aspect;

import cn.hutool.core.collection.CollUtil;
import com.szzt.iot.admin.common.annotation.DataFilter;
import com.szzt.iot.admin.common.interceptor.DataScope;
import com.szzt.iot.common.constant.Constant;
import com.szzt.iot.common.exception.ErrorCode;
import com.szzt.iot.admin.modules.security.user.SecurityUser;
import com.szzt.iot.admin.modules.security.user.UserDetail;
import com.szzt.iot.admin.modules.sys.enums.SuperAdminEnum;
import com.szzt.iot.common.exception.RobotException;
import org.apache.commons.lang3.StringUtils;
import org.aspectj.lang.JoinPoint;
import org.aspectj.lang.annotation.Aspect;
import org.aspectj.lang.annotation.Before;
import org.aspectj.lang.annotation.Pointcut;
import org.aspectj.lang.reflect.MethodSignature;
import org.springframework.stereotype.Component;

import java.lang.reflect.Method;
import java.util.List;
import java.util.Map;

/**
 * 数据过滤，切面处理类
 *
 * @author
 */
@Aspect
@Component
public class DataFilterAspect {

    @Pointcut("@annotation( com.szzt.iot.admin.common.annotation.DataFilter)")
    public void dataFilterCut() {

    }

    @Before("dataFilterCut()")
    public void dataFilter(JoinPoint point) {
        Object params = point.getArgs()[0];
        if (params != null && params instanceof Map) {
            UserDetail user = SecurityUser.getUser();

            //如果是超级管理员，则不进行数据过滤
            if (user.getSuperAdmin() == SuperAdminEnum.YES.value()) {
                return;
            }

            try {
                //否则进行数据过滤
                Map map = (Map) params;
                String sqlFilter = getSqlFilter(user, point);
                map.put(Constant.SQL_FILTER, new DataScope(sqlFilter));
            } catch (Exception e) {

            }

            return;
        }

        throw new RobotException(ErrorCode.DATA_SCOPE_PARAMS_ERROR);
    }

    /**
     * 获取数据过滤的SQL
     */
    private String getSqlFilter(UserDetail user, JoinPoint point) throws Exception {
        MethodSignature signature = (MethodSignature) point.getSignature();
        Method method = point.getTarget().getClass().getDeclaredMethod(signature.getName(), signature.getParameterTypes());
        DataFilter dataFilter = method.getAnnotation(DataFilter.class);

        //获取表的别名
        String tableAlias = dataFilter.tableAlias();
        if (StringUtils.isNotBlank(tableAlias)) {
            tableAlias += ".";
        }

        StringBuilder sqlFilter = new StringBuilder();

        //查询条件前缀
        String prefix = dataFilter.prefix();
        if (StringUtils.isNotBlank(prefix)) {
            sqlFilter.append(" ").append(prefix);
        }

        sqlFilter.append(" (");

        //部门ID列表
        List<Long> deptIdList = user.getDeptIdList();
        if (CollUtil.isNotEmpty(deptIdList)) {
            sqlFilter.append(tableAlias).append(dataFilter.deptId());

            sqlFilter.append(" in(").append(StringUtils.join(deptIdList, ",")).append(")");
        }

        //查询本人数据
        if (CollUtil.isNotEmpty(deptIdList)) {
            sqlFilter.append(" or ");
        }
        sqlFilter.append(tableAlias).append(dataFilter.userId()).append("=").append(user.getId());

        sqlFilter.append(")");

        return sqlFilter.toString();
    }
}